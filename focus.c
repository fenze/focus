#include "focus.h"
#include <stdio.h>

char *untilde_path(char *path)
{
	char *realpath = getenv("HOME");
	strcat(realpath, ++path);
	return realpath;
}

void error(int argc, ...)
{
	va_list args;
	va_start(args, argc);
	fputs("\033[0;31merror\033[0m: ", stderr);

	while (argc--)
		fputs(va_arg(args, char*), stderr);

	fputs(".\n", stderr);
	va_end(args);
}

int timer_status()
{
	FILE *fp = fopen(TMPF, "r");
	int status = (fp) ? 0 : 1;
	if (fp)
		fclose(fp);
	return status;
}

int timer_is_paused()
{
	int i = 0;
	char buf[11];
	FILE *fp = fopen(TMPF, "r");
	while (fgets(buf, 11, fp)) ++i;
	if (fp) fclose(fp);
	return (i % 2) ? 0 : 1;
}


int timer_start()
{
	if (!timer_status())
	{
		if (timer_is_paused())
		{
			FILE *fp = fopen(TMPF, "a");
			fprintf(fp, "%li", time(0));
			fclose(fp);
			return 0;
		}

		return 1;
	}

	FILE *fp = fopen(TMPF, "w");
	fprintf(fp, "%li", time(0));
	fclose(fp);
	return 0;
}

int timer_pause()
{
	int status;

	if ((status = timer_status()))
		return 2;

	if ((status = timer_is_paused()))
		return 3;

	FILE *fp = fopen(TMPF, "a");
	fprintf(fp, "%li", time(0));
	fclose(fp);
	return status;
}

int timer_time()
{
	char buf[11];

	FILE *fp = fopen(TMPF, "r");

	int i, last, res = 0;
	for (i = 2; fgets(buf, 11, fp); i++)
	{
		if (i % 2 == 0)
		{
			last = atoi(buf);
			continue;
		}

		res += atoi(buf) - last;
	}

	if (i % 2)
		res += time(0) - last;

	fclose(fp);
	return res;
}

int timer_stop()
{
	char msg[128];
	if (timer_status())
		return 2;

	timer_pause();

	FILE *fp = fopen("/tmp/COMMIT_EDITMSG", "w");
	fputs("\n# Please enter the title for your session.\n"
			"# See manpage for more informations.", fp);
	fclose(fp);

	fp = fopen("/tmp/COMMIT_EDITMSG", "r");

	system("/bin/sh -c '$EDITOR /tmp/COMMIT_EDITMSG'");

	fgets(msg, 128, fp);

	if (fp)
		fclose(fp);
	if (strlen(msg) == 1)
		return 4;

	char *hist = untilde_path("~/.local/share/focus/history");

	fp = fopen(hist, "a");

	fprintf(fp, "Title: %sTime:", msg);

	time_t hours, minutes, seconds;
	time_t timer = timer_time();

	timer -= ((hours = timer / 3600)) * 3600;
	timer -= ((minutes = timer / 60)) * 60;
	seconds = timer % 60;

	if (hours)
		fprintf(fp, " %lu hour(s)", hours);

	if (minutes)
		fprintf(fp, " %lu minute(s)", minutes);

	if (seconds)
		fprintf(fp, " %lu second(s)", seconds);

	fputs(".", fp);

	fclose(fp);

	remove(TMPF);
	remove("/tmp/COMMIT_EDITMSG");
	return 0;
}

int timer_show()
{
	if (timer_status())
		return 2;

	time_t hours, minutes, seconds = 0;
	time_t timer = timer_time();

	if (!timer)
	{
		char buf[11];
		FILE *fp = fopen(TMPF, "r");
		fgets(buf, 11, fp);

		timer = time(0) - atoi(buf);

		if (fp)
			fclose(fp);
	}

	timer -= ((hours = timer / 3600)) * 3600;
	timer -= ((minutes = timer / 60)) * 60;
	seconds = timer % 60;

	if (timer_is_paused())
		printf("\033[0;31m•\033[0m Paused\n  Total:");
	else if (!timer_status())
		printf("\033[0;32m•\033[0m Running\n  Total:");

	if (hours)
		printf(" %lu hour(s)", hours);

	if (minutes)
		printf(" %lu minute(s)", minutes);

	if (seconds)
		printf(" %lu second(s)", seconds);

	puts(".");

	return 0;
}

void help()
{
	puts("usage: focus [-hv] [action]\n\n"
			 " start\t\t - starts session");
}

int main(int argc, char **argv)
{
	argc--; argv++;

	int exit_code = 0;

	for (int i = 0; i < argc; i++)
	{
		if (argv[i][0] == '-')
				switch (argv[i][1])
				{
					case 'v':
						puts("focus v1.0");
						break;
					case 'h':
						help();
						break;
					default:
						error(3, "invalid option -- '", argv[i], "'");
				}
		else
			if (!strcmp(argv[i], "start"))
				exit_code = timer_start();
			else if (!strcmp(argv[i], "pause"))
				exit_code = timer_pause();
			else if (!strcmp(argv[i], "status"))
				exit_code = timer_show();
			else if (!strcmp(argv[i], "stop"))
				exit_code = timer_stop();
			else
				error(3, "invalid action -- '", argv[i], "'");
	}

	switch (exit_code)
	{
		case 1:
			error(1, "timer is running");
			break;
		case 2:
			error(1, "timer is not running");
			break;
		case 3:
			error(1, "timer is paused");
			break;
		case 4:
			error(1, "Aborting due to empty title");;
	}

	return exit_code;
}
