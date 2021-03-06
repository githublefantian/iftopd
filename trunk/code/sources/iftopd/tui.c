/*
 * tui.c:
 *
 * Based on ui.c from the original iftop sources.
 *
 * This user interface does not make use of curses. Instead, it prints its
 * output to STDOUT. This output is activated by providing the '-t' flag.
 *
 */

#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>

#include "sorted_list.h"
#include "options.h"
#include "ui_common.h"

/* Width of the host column in the output */
#define PRINT_WIDTH 40
#define EPSILON 0.0000001

extern FILE *fp_log;


/*
 * UI print function
 */
void tui_print() {
  sorted_list_node* nn = NULL;
  char host1[HOSTNAME_LENGTH], host2[HOSTNAME_LENGTH];
  char buf0_10[10], buf1_10[10], buf2_10[10];
  char host_temp[HOSTNAME_LENGTH_FOR_LOG]= {'\0'};
  int j;
  static char *label;
  static char *labellong;
  int l = 0;
  float total_speed_temp = 0;
  

  if (!label) {
    xfree(label);
    label = (char *)calloc(PRINT_WIDTH + 1, 1);
  }

  if (!labellong) {
    xfree(labellong);
    labellong = (char *)calloc(PRINT_WIDTH + 1 + 9, 1);
  }

  if (options.paused ) {
    return;
  }


#if 0
		time_t timer_log;//time_t就是long int 类
		struct tm *tblock =NULL;  
		char * temp_block = NULL;
		timer_log = time(NULL);//这一句也可以改成time(&timer);	
		tblock = localtime(&timer_log);
		//temp_block = " ^^^";
		//temp_block = asctime(tblock);
		//fprintf(fp_log,"records time is: %s\n\n",temp_block);//
		fprintf(fp_log,"Beijing time: %d\n",timer_log);
													
#endif


  /* Headings */
  snprintf(label, PRINT_WIDTH, "%-*s", PRINT_WIDTH, "Host name (port/service if enabled)");
  printf("%s %s     %10s %10s %10s %10s\n", "   #", label, "last 2s", "last 10s", "last 40s", "cumulative");
  //fprintf(fp_log,"%s %s     %10s %10s %10s %10s\n", "   #", label, "last 2s", "last 10s", "last 40s", "cumulative");
  fprintf(fp_log,"%6s      %-6s ", "date","time");
  fprintf(fp_log,"% 4s  ", "num");
  fprintf(fp_log,"%-20s","local_ip");
  fprintf(fp_log,"%4s", " ");
  fprintf(fp_log,"%-15s","remot_ip");
  fprintf(fp_log,"%13s", " ");
  fprintf(fp_log,"%s","up");
  fprintf(fp_log,"%11s", " ");
  fprintf(fp_log,"%-s","down");
  fprintf(fp_log,"%10s", " ");
  fprintf(fp_log,"%s","total");
  fprintf(fp_log,"\n");

  /* Divider line */
  for (j = 0; j < PRINT_WIDTH + 52; j++) {
    printf("-");
   //fprintf(fp_log,"-");
  }
  printf("\n");
  fprintf(fp_log,"\n");

  /* Traverse the list of all connections */
  while((nn = sorted_list_next_item(&screen_list, nn)) != NULL && l < options.num_lines) {
  
    /* Get the connection information */
    host_pair_line* screen_line = (host_pair_line*)nn->data;

#if 1 /* filter */
	readable_size(screen_line->sent[0]+screen_line->recv[0], buf0_10, 10, 1024, options.bandwidth_in_bytes);	

	//fprintf(fp_log,"*** %10s****", buf0_10);
	total_speed_temp = atof(buf0_10);
	//fprintf(fp_log,"+++%f+++", total_speed_temp);
	
	if(-EPSILON < total_speed_temp && total_speed_temp < EPSILON){
		continue;
	}
#endif
	/* Increment the line counter */
	   l++;

    /* Assemble host information */
    sprint_host(host1, screen_line->ap.af, &(screen_line->ap.src6), screen_line->ap.src_port, screen_line->ap.protocol, PRINT_WIDTH, options.aggregate_src);
    sprint_host(host2, screen_line->ap.af, &(screen_line->ap.dst6), screen_line->ap.dst_port, screen_line->ap.protocol, PRINT_WIDTH, options.aggregate_dest);

	
#if 1
	//char *wday[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	time_t timep;
	struct tm *p;
	time(&timep); /*获得time_t结构的时间，UTC时间*/
	p = localtime(&timep); /*转换为struct tm结构的UTC时间*/
	fprintf(fp_log,"%d-%d-%d ", 1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday);
	fprintf(fp_log,"%d:%d:%d ", p->tm_hour,p->tm_min, p->tm_sec);
													  
#endif	
	
    /* Send rate per connection */
    printf("%4d %s%s", l, host1, " =>");
	
	strncpy(host_temp,host1,HOSTNAME_LENGTH_FOR_LOG);
	host_temp[HOSTNAME_LENGTH_FOR_LOG-1] ='\0';
	//fprintf(fp_log,"%4d %s%s", l, host_temp, "<=>");
	fprintf(fp_log,"  %d. %s%s",l, host_temp, "<=>");
	
	
	strncpy(host_temp,host2,HOSTNAME_LENGTH_FOR_LOG);
	host_temp[HOSTNAME_LENGTH_FOR_LOG-1] ='\0';
	fprintf(fp_log," %s%s", host_temp, " ");
	

/* 发送 */

	if(1 == options.speed_second_time)
	{
		for(j = 0; j < HISTORY_DIVISIONS-2; j++) {
	      readable_size(screen_line->sent[j], buf0_10, 10, 1024, options.bandwidth_in_bytes);
	      printf(" %10s", buf0_10);
		  fprintf(fp_log," %10s", buf0_10);
		  fprintf(fp_log,"%-4s","\/s");
		  //fprintf(fp_log,"%-4s","鈫?);
    	}
	}
	else if(2 == options.speed_second_time)
	{
		for(j = 2; j < HISTORY_DIVISIONS; j++) {
	      readable_size(screen_line->sent[j], buf0_10, 10, 1024, options.bandwidth_in_bytes);
	      printf(" %10s", buf0_10);
		  fprintf(fp_log," %10s", buf0_10);
		  fprintf(fp_log,"%-4s","\/Min");
		  //fprintf(fp_log,"%-4s","鈫?);
    	}
	}
	
	
    /* Cumulative sent data per connection */
    readable_size(screen_line->total_sent, buf0_10, 10, 1024, 1);
    printf(" %10s\n", buf0_10);
	//fprintf(fp_log," %10s\n", buf0_10);

    /* Receive rate per connection */
    printf("     %s%s", host2, " <=");
	//fprintf(fp_log,"     %s%s", host2, " <=");

/* 接收 */	
	if(1 == options.speed_second_time)
	{
		for(j = 0; j < HISTORY_DIVISIONS-2; j++) {
	      readable_size(screen_line->recv[j], buf0_10, 10, 1024, options.bandwidth_in_bytes);
	      printf(" %10s", buf0_10);
		  fprintf(fp_log," %10s", buf0_10);
		  fprintf(fp_log,"%-4s","\/s");
		  //fprintf(fp_log,"%-4s","鈫?);
    	}
	}
	else if(2 == options.speed_second_time)
	{
		for(j = 2; j < HISTORY_DIVISIONS; j++) {
	      readable_size(screen_line->recv[j], buf0_10, 10, 1024, options.bandwidth_in_bytes);
	      printf(" %10s", buf0_10);
		  fprintf(fp_log," %10s", buf0_10);
		  fprintf(fp_log,"%-4s","\/Min");
		  //fprintf(fp_log,"%-4s","鈫?);
    	}
	}
    

/* 发送   + 接收*/ 
#if 1
	if(1 == options.speed_second_time)
	{
		for(j = 0; j < HISTORY_DIVISIONS-2; j++) {
	      readable_size(screen_line->sent[j]+screen_line->recv[j], buf0_10, 10, 1024, options.bandwidth_in_bytes);
		  fprintf(fp_log," %10s", buf0_10);
		  fprintf(fp_log,"%-4s","\/s");
	    }
	}else if(2 == options.speed_second_time)
	{
		for(j = 2; j < HISTORY_DIVISIONS; j++) {
	      readable_size(screen_line->sent[j]+screen_line->recv[j], buf0_10, 10, 1024, options.bandwidth_in_bytes);
		  fprintf(fp_log," %10s", buf0_10);
		  fprintf(fp_log,"%-4s","\/Min");
	    }
	}
	

#endif
	
	
	fprintf(fp_log,"\n");

	
    /* Cumulative received data per connection */
    readable_size(screen_line->total_recv, buf0_10, 10, 1024, 1);
    printf(" %10s\n", buf0_10);
	//fprintf(fp_log," %10s\n", buf0_10);
  }

  /* Divider line */
  for (j = 0; j < PRINT_WIDTH + 52; j++) {
    printf("-");
	//fprintf(fp_log,"-");
  }
  fprintf(fp_log,"\n");
  printf("\n");

  /* Rate totals */
  snprintf(labellong, PRINT_WIDTH + 9, "%-*s", PRINT_WIDTH + 9, "Total send rate:");
  printf("%s ", labellong);
  for(j = 0; j < HISTORY_DIVISIONS; j++) {
    readable_size((((host_pair_line *)&totals)->sent[j]) , buf0_10, 10, 1024, options.bandwidth_in_bytes);
    printf("%10s%c", buf0_10, j == HISTORY_DIVISIONS - 1 ? '\n' : ' ');
  }

  snprintf(labellong, PRINT_WIDTH + 9, "%-*s", PRINT_WIDTH + 9, "Total receive rate:");
  printf("%s ", labellong);
  for(j = 0; j < HISTORY_DIVISIONS; j++) {
    readable_size((((host_pair_line *)&totals)->recv[j]) , buf0_10, 10, 1024, options.bandwidth_in_bytes);
    printf("%10s%c", buf0_10, j == HISTORY_DIVISIONS - 1 ? '\n' : ' ');
  }

  snprintf(labellong, PRINT_WIDTH + 9, "%-*s", PRINT_WIDTH + 9, "Total send and receive rate:");
  printf("%s ", labellong);
  for(j = 0; j < HISTORY_DIVISIONS; j++) {
    readable_size((((host_pair_line *)&totals)->sent[j] + ((host_pair_line *)&totals)->recv[j]) , buf0_10, 10, 1024, options.bandwidth_in_bytes);
    printf("%10s%c", buf0_10, j == HISTORY_DIVISIONS - 1 ? '\n' : ' ');
  }

  /* Divider line */
  for (j = 0; j < PRINT_WIDTH + 52; j++) {
    printf("-");
  }
  printf("\n");

  /* Peak traffic */
  snprintf(labellong, PRINT_WIDTH + 9, "%-*s", PRINT_WIDTH + 9, "Peak rate (sent/received/total):");
  readable_size(peaksent / RESOLUTION, buf0_10, 10, 1024, options.bandwidth_in_bytes);
  readable_size(peakrecv / RESOLUTION, buf1_10, 10, 1024, options.bandwidth_in_bytes);
  readable_size(peaktotal / RESOLUTION, buf2_10, 10, 1024, options.bandwidth_in_bytes);
  printf("%s %10s %10s %10s\n", labellong, buf0_10, buf1_10, buf2_10);

  /* Cumulative totals */
  snprintf(labellong, PRINT_WIDTH + 9, "%-*s", PRINT_WIDTH + 9, "Cumulative (sent/received/total):");
  readable_size(history_totals.total_sent, buf0_10, 10, 1024, 1);
  readable_size(history_totals.total_recv, buf1_10, 10, 1024, 1);
  readable_size(history_totals.total_recv + history_totals.total_sent, buf2_10, 10, 1024, 1);
  printf("%s %10s %10s %10s\n", labellong, buf0_10, buf1_10, buf2_10);

  /* Double divider line */
  for (j = 0; j < PRINT_WIDTH + 52; j++) {
    printf("=");
  }
  printf("\n\n");
  fprintf(fp_log,"\n\n\n");
}


/*
 * Text interface data structure initializations.
 */
void tui_init() {
  screen_list_init();
  screen_hash = addr_hash_create();
  service_hash = serv_hash_create();
  serv_hash_initialise(service_hash);

  printf("Listening on %s\n", options.interface);
  fprintf(fp_log,"Listening on %s\n", options.interface);
}


/*
 * Tick function indicating screen refresh
 */
void tui_tick(int print) {
  if (print) {
    tui_print();
  }
}


/*
 * Main UI loop. Code any interactive character inputs here.
 */
void tui_loop() {
  int i;
  extern sig_atomic_t foad;

  system("/bin/stty cbreak -echo >/dev/null 2>&1");
  while ((i = getchar()) != 'q' && foad == 0) {
    switch (i) {
      case 'u':
        tick(1);
        break;
      case 'n':
        options.dnsresolution ^= 1;
        printf("DNS resolution is %s.\n\n", options.dnsresolution ? "ON" : "OFF");
        tick(1);
        break;
      case 'N':
        options.portresolution ^= 1;
        printf("Port resolution is %s.\n\n", options.portresolution ? "ON" : "OFF");
        tick(1);
        break;
      case 's':
        options.aggregate_src ^= 1;
        printf("%s source host\n\n", options.aggregate_src ? "Hide" : "Show");
        tick(1);
        break;
      case 'd':
        options.aggregate_dest ^= 1;
        printf("%s destination host\n\n", options.aggregate_dest ? "Hide" : "Show");
        tick(1);
        break;
      case 'S':
        if (options.showports == OPTION_PORTS_OFF) {
          options.showports = OPTION_PORTS_SRC;
        }
        else if (options.showports == OPTION_PORTS_DEST) {
          options.showports = OPTION_PORTS_ON;
        }
        else if(options.showports == OPTION_PORTS_ON) {
          options.showports = OPTION_PORTS_DEST;
        }
        else {
          options.showports = OPTION_PORTS_OFF;
        }
        printf("Showing ports:%s%s%s%s.\n\n",
          options.showports == OPTION_PORTS_SRC ? " src" : "",
          options.showports == OPTION_PORTS_DEST ? " dest" : "",
          options.showports == OPTION_PORTS_ON ? " both" : "",
          options.showports == OPTION_PORTS_OFF ? " none" : "");
        tick(1);
        break;
      case 'D':
        if (options.showports == OPTION_PORTS_OFF) {
          options.showports = OPTION_PORTS_DEST;
        }
        else if (options.showports == OPTION_PORTS_SRC) {
          options.showports = OPTION_PORTS_ON;
        }
        else if(options.showports == OPTION_PORTS_ON) {
          options.showports = OPTION_PORTS_SRC;
        }
        else {
          options.showports = OPTION_PORTS_OFF;
        }
        printf("Showing ports:%s%s%s%s.\n\n",
          options.showports == OPTION_PORTS_SRC ? " src" : "",
          options.showports == OPTION_PORTS_DEST ? " dest" : "",
          options.showports == OPTION_PORTS_ON ? " both" : "",
          options.showports == OPTION_PORTS_OFF ? " none" : "");
        tick(1);
        break;
      case 'p':
        options.showports =
         (options.showports == OPTION_PORTS_OFF) ?
          OPTION_PORTS_ON :
          OPTION_PORTS_OFF;
        printf("Showing ports:%s%s%s%s.\n\n",
          options.showports == OPTION_PORTS_SRC ? " src" : "",
          options.showports == OPTION_PORTS_DEST ? " dest" : "",
          options.showports == OPTION_PORTS_ON ? " both" : "",
          options.showports == OPTION_PORTS_OFF ? " none" : "");
        tick(1);
        break;
      case 'P':
        options.paused ^= 1;
        if (options.paused) {
          printf("Pausing... press 'P' again to continue.\n");
        }
	else {
	  printf("Continuing.\n\n");
	  tick(1);
	}
	break;
      case 'o':
        options.freezeorder ^= 1;
        printf("Order %s.\n\n", options.freezeorder ? "frozen" : "unfrozen");
	tick(1);
	break;
      case '1':
        options.sort = OPTION_SORT_DIV1;
        printf("Sorting by column 1.\n\n");
	tick(1);
        break;
      case '2':
        options.sort = OPTION_SORT_DIV2;
        printf("Sorting by column 2.\n\n");
        tick(1);
        break;
      case '3':
        options.sort = OPTION_SORT_DIV3;
        printf("Sorting by column 3.\n\n");
        tick(1);
        break;
      case '<':
        options.sort = OPTION_SORT_SRC;
        printf("Sorting by column source.\n\n");
        tick(1);
        break;
      case '>':
        options.sort = OPTION_SORT_DEST;
        printf("Sorting by column destination.\n\n");
        tick(1);
        break;
      default:
        break;
    }
  }
  endloop:
  system("/bin/stty -cbreak echo >/dev/null 2>&1");
}

