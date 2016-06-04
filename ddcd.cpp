/*
This software is part of libcsdr, a set of simple DSP routines for 
Software Defined Radio.

Copyright (c) 2014, Andras Retzler <randras@sdr.hu>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ANDRAS RETZLER BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "ddcd.h"

int host_port = 0;
char host_address[100] = "127.0.0.1";
int decimation = 0;
float transition_bw = 0.05;
int bufsize = 1024;
int bufcnt = 1024;
int maxbufcnt = 1000
int thread_cntr = 0;
char ddc_method_str[100] = "td";
ddc_method_t ddc_method;

void sig_handler(int signo)
{	
	fprintf(stderr, MSG_START "signal %d caught, exiting ddcd...\n", signo);
	fflush(stderr);
	exit(0);
}

int main(int argc, char* argv[])
{
	int c;
	for(;;)
	{
		int option_index = 0;
		static struct option long_options[] = {
		   {"port",       required_argument, 0,  'p' },
		   {"address",    required_argument, 0,  'a' },
		   {"decimation", required_argument, 0,  'd' },
		   {"bufsize", 	  required_argument, 0,  'b' },
	       {"method", 	  required_argument, 0,  'm' },
	       {"transition", required_argument, 0,  't' }
		};
		c = getopt_long(argc, argv, "p:a:d:b:m:t:", long_options, &option_index);
		if(c==-1) break;
		switch (c) 
		{
		case 'a':
			host_address[100-1]=0;
			strncpy(host_address,optarg,100-1);
			break;
		case 'p':
			host_port=atoi(optarg);
			break;
		case 'd':
			decimation=atoi(optarg);
			break;
		case 'b':
			bufsize=atoi(optarg);
			break;
		case 'm':
			ddc_method_str[100-1]=0;
			strncpy(ddc_method_str,optarg,100-1);
			break;
		case 't':
			sscanf(optarg,"%g",&transition_bw);
			break;
		case 0:
		case '?':
		case ':':
		default:;
			print_exit(MSG_START "error in getopt_long()\n");
		}
	}
	
	if(!decimation) print_exit(MSG_START "missing required command line argument, --decimation.\n");
	if(!host_port) print_exit(MSG_START "missing required command line argument, --port.\n");
	if(decimation<0) print_exit(MSG_START "invalid value for --decimation (should be >0).\n");
	if(decimation==1) fprintf(stderr, MSG_START "decimation = 1, just copying raw samples.\n");
	if(transition_bw<0||transition_bw>0.5) print_exit(MSG_START "invalid value for --transition (should be between 0 and 0.5).\n");
	
	if(decimation==1); //don't do anything then
	else if(!strcmp(ddc_method_str,"td")) 
	{
		ddc_method = M_TD; 
		fprintf(stderr, MSG_START "method is M_TD (default).\n");
	}
	else if (!strcmp(ddc_method_str,"fastddc")) 
	{
		ddc_method = M_FASTDDC; 
		fprintf(stderr, MSG_START "method is M_FASTDDC.\n");
	}
	else print_exit(MSG_START "invalid parameter given to --method.\n");

	//set signals
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sig_handler;
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGKILL, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGHUP, &sa, NULL);
	
	struct sockaddr_in addr_host;
    int listen_socket;
	std::vector<client_t*> clients;
	clients.reserve(100);
    listen_socket=socket(AF_INET,SOCK_STREAM,0);

	int sockopt = 1;
	if( setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&sockopt, sizeof(sockopt)) == -1 )
		error_exit(MSG_START "cannot set SO_REUSEADDR");  //the best description on SO_REUSEADDR ever: http://stackoverflow.com/a/14388707/3182453
	
	memset(&addr_host,'0',sizeof(addr_host));
    addr_host.sin_family = AF_INET;
    addr_host.sin_port = htons(host_port);
	addr_host.sin_addr.s_addr = INADDR_ANY;

    if( (addr_host.sin_addr.s_addr=inet_addr(host_address)) == INADDR_NONE ) 
		error_exit(MSG_START "invalid host address");

	if( bind(listen_socket, (struct sockaddr*) &addr_host, sizeof(addr_host)) < 0 )
		error_exit(MSG_START "cannot bind() address to the socket");

	if( listen(listen_socket, 10) == -1 )
		error_exit(MSG_START "cannot listen() on socket");

	fprintf(stderr,MSG_START "listening on %s:%d\n", inet_ntoa(addr_host.sin_addr), host_port);

	struct sockaddr_in addr_cli;
	socklen_t addr_cli_len = sizeof(addr_cli);
	int new_socket;

	int highfd = 0; 
	FD_ZERO(&select_fds);
	FD_SET(listen_socket, &select_fds);
	maxfd(&highfd, listen_socket);
	FD_SET(input_fd, &select_fds);
	maxfd(&highfd, input_fd);

	//Set stdin and listen_socket to non-blocking 
	if(set_nonblocking(input_fd) || set_nonblocking(listen_socket)) //don't do it before subprocess fork!
		error_exit(MSG_START "cannot set_nonblocking()");

	for(;;)
	{
		//Let's wait until there is any new data to read, or any new connection!
		select(highfd, &select_fds, NULL, NULL, NULL);

		//Is there a new client connection?
		if( (new_socket = accept(listen_socket, (struct sockaddr*)&addr_cli, &addr_cli_len)) != -1)
		{ 
			this_client = new client_t;
			this_client->error = 0;
			memcpy(&this_client->addr, &addr_cli, sizeof(this_client->addr));
			this_client->socket = new_socket;
			this_client->id = thread_cntr++;
			
			if(pthread_create(&this_client->thread, NULL, client_thread , (void*)&this_client)<0)
			{
				//We're the parent
				clients.push_back(this_client);
				fprintf(stderr, MSG_START "pthread_create() done, this_client->id: %d\n", this_client->id);
			}
		}

		float* pool_next = pool->get_write_buffer();

		int retval = read(input_fd, pool_next, mainpool->size);
		if(retval==0)
		{
			//end of input stream, close clients and exit
		}
		else if(retval != -1)
		{
			for (int i=0; i<clients.size(); i++)
			{
				if(write(clients[i]->pipefd[1], buf, retval)==-1)
				{
					
					if(!clients[i]->error) 
					{
						print_client(clients[i], "lost buffer, failed to write pipe.");
						clients[i]->error=1;
					}
					//fprintf(stderr, MSG_START "errno is %d\n", errno); //usually 11
					//int wpstatus;
					//int wpresult = waitpid(clients[i]->pid, &wpstatus, WNOHANG);
					//fprintf(stderr, MSG_START "pid is %d\n",clients[i]->pid);
					//perror("somethings wrong");
					//if(wpresult == -1) print_client(clients[i], "error while waitpid()!");
					//else if(wpresult == 0) 
					waitpid(clients[i]->pid, NULL, WNOHANG);
					if(!proc_exists(clients[i]->pid))
					{
						//Client exited!
						print_client(clients[i], "closing client from main process.");
						close(clients[i]->pipefd[1]);
						close(clients[i]->socket);
						delete clients[i];
						clients.erase(clients.begin()+i);
						fprintf(stderr, MSG_START "done closing client from main process.\n");
					}
				}
				else  { if(clients[i]->error) print_client(clients[i], "pipe okay again."); clients[i]->error=0; }
			}
		}
		//TODO: at the end, server closes pipefd[1] for client

	}


}

void* client_thread (void* param)
{
}

void error_exit(const char* why)
{
	perror(why);
	exit(1);
}

void print_exit(const char* why)
{
	fprintf(stderr, "%s", why);
	exit(1);
}

void maxfd(int* maxfd, int fd)
{
	if(fd>=*maxfd) *maxfd=fd+1; 
}

