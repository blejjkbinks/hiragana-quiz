#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <time.h>

static void	str_to_index(char *str, char *c, char *v)
{
	char	*dup = NULL;

	if (strcmp(str, "fu") == 0)
		dup = "hu";
	else if (strcmp(str, "ji") == 0)
		dup = "zi";
	else if (strcmp(str, "tsu") == 0)
		dup = "tu";
	else if (strcmp(str, "chi") == 0)
		dup = "ti";
	else if (strcmp(str, "shi") == 0)
		dup = "si";
	else if (strlen(str) > 2)
		dup = "00";
	if (!dup)
	{
		if (strlen(str) == 1)
		{
			*c = '-';
			*v = str[0];
		}
		else
		{
			*c = str[0];
			*v = str[1];
		}
	}
	else
	{
		*c = dup[0];
		*v = dup[1];
	}
}

//i and j are chars from "-aeuio" and "kst..." etc
static void	index_to_str(char *str, int i, int j)
{
	str[0] = i;
	str[1] = j;
	str[2] = 0;
	if (str[0] == '-')
	{
		str[0] = str[1];
		str[1] = 0;
	}
	if (strcmp(str, "hu") == 0)
		str[0] = 'f';
	if (strcmp(str, "zi") == 0)
		str[0] = 'j';
	if (strcmp(str, "tu") == 0)
	{
		str[0] = 't';
		str[1] = 's';
		str[2] = 'u';
		str[3] = 0;
	}
	else if (strcmp(str, "ti") == 0)
	{
		str[0] = 'c';
		str[1] = 'h';
		str[2] = 'i';
		str[3] = 0;
	}
	else if (strcmp(str, "si") == 0)
	{
		str[0] = 's';
		str[1] = 'h';
		str[2] = 'i';
		str[3] = 0;
	}
}

static wchar_t	*strwchr(wchar_t *str, wchar_t c, int size)
{
	int	i = 0;

	while (i < size)
	{
		if (str[i] == c)
			return (&str[i]);
		i++;
	}
	return (NULL);
}

static wchar_t	romaji_to_kana(char *str)
{
	char	c = '0', v = '0';
	char	*cons, *vows;
	wchar_t	data[15][5] = {
		{0x3042, 0x3044, 0x3046, 0x3048, 0x304A},
		{0x304B, 0x304D, 0x304F, 0x3051, 0x3053},
		{0x304C, 0x304E, 0x3050, 0x3052, 0x3054},
		{0x3055, 0x3057, 0x3059, 0x305B, 0x305D},
		{0x3056, 0x3058, 0x305A, 0x305C, 0x305E},
		{0x305F, 0x3061, 0x3063, 0x3066, 0x3068},
		{0x3060, 0x3062, 0x3065, 0x3067, 0x3069},
		{0x306A, 0x306B, 0x306C, 0x306D, 0x306E},
		{0x306F, 0x3072, 0x3075, 0x3078, 0x307B},
		{0x3070, 0x3073, 0x3076, 0x3079, 0x307C},
		{0x3071, 0x3074, 0x3077, 0x307A, 0x307D},
		{0x307E, 0x307F, 0x3080, 0x3081, 0x3082},
		{0x3084, 0, 0x3086, 0, 0x3088},
		{0x3089, 0x308A, 0x308B, 0x308C, 0x308D},
		{0x308F, 0x3090, 0, 0x3091, 0x3092}};

	if (strcmp(str, "hu") * strcmp(str, "tu") * strcmp(str, "zi") == 0)
		return (0);
	else if (strcmp(str, "ti") * strcmp(str, "si") == 0)
		return (0);
	if (str[0] == '-')
		return (0);
	if (strcmp(str, "n") == 0)
		return (0x3093);
	str_to_index(str, &c, &v);
	//printf("asked for \"%c%c\": ", c, v);
	cons = "-kgsztdnhbpmyrw";
	c = (char)(strchr(cons, c) - cons);
	vows = "aiueo";
	v = (char)(strchr(vows, v) - vows);
	if (0 <= c && c <= 14 && 0 <= v && v <= 4)
	{
		//printf("looking for table[%d][%d]\n", c, v);
		return (data[(int)c][(int)v]);
	}
	else
	{
		//printf("[%d][%d] invald, out of table\n", c, v);
		return (0);
	}
}



static char	*kana_to_romaji(wchar_t kana)
{
	char	*str = (char *)malloc(20 * sizeof(char));
	char	*cons = "-kgsztdnhbpmyrw";
	char	*vows = "aiueon";

	for (int i = 0; i < (int)strlen(cons); i++)
	{
		for (int j = 0; j < (int)strlen(vows); j++)
		{
			index_to_str(str, cons[i], vows[j]);
			if (romaji_to_kana(str) == kana)
				return (str);
		}
	}
	return (NULL);
}

static wchar_t	random_kana(void)
{
	char	*str;	
	char	*cons = "-kstnhmyrw";
	char	*vows = "aiueon";
	int		i, j;
	wchar_t	kana;
	static wchar_t	*history;
	int	history_size = 20;

	if (!history)
	{
		history = (wchar_t *)malloc(history_size * sizeof(wchar_t));
		history[0] = '.';
	}
	while (1)
	{
		i = rand() % strlen(cons);
		j = rand() % strlen(vows);
		str = (char *)malloc(20 * sizeof(char));
		index_to_str(str, cons[i], vows[j]);
		kana = romaji_to_kana(str);
		free(str);
		if (kana)
		{
			if (!strwchr(history, kana, history_size))
			{
				i = strwchr(history, '.', history_size) - history;
				j = (i + 1) % history_size;
				history[i] = kana;
				history[j] = '.';
				/*printf("\n");
				for (int k = 0; k < history_size; k++)
				{
					if (history[k] == '.')
						printf(" ");
					printf("%lc", history[k]);
				}*/
				return (kana);
			}
		}
	}
}

static void	test_random(int l)
{
	wchar_t	kana;
	int	rh_size = 74;
	wchar_t	*rh = (wchar_t *)calloc(rh_size, sizeof(wchar_t));
	int	*count = (int *)calloc(rh_size, sizeof(int));
	for (int i = 0; i < l; i++)
	{
		int	j;
		kana = random_kana();
		if (!strwchr(rh, kana, rh_size))
		{
			j = strwchr(rh, 0, rh_size) - rh;
			rh[j] = kana;
		}
		j = strwchr(rh, kana, rh_size) - rh;
		count[j]++;
		/*for (int k = 0; k < rh_size; k++)
			printf(" %lc", rh[k]);
		printf("\n");
		for (int k = 0; k < rh_size; k++)
			printf("%3d", count[k]);*/
	}
	int j = strwchr(rh, 0, rh_size) - rh;
	printf("number of chars: %d\n", j);
	for (int i = 0; i < rh_size; i++)
	{
		char *str = kana_to_romaji(rh[i]);
		printf("%5d ", count[i]);
		printf("%lc", rh[i]);
		printf("%3s\n", str);
		free(str);
	}
	free(rh);
}

static void	print_sugg(wchar_t kana, int count, int wallet)
{
	if (!(wallet >= 0))
	{
		printf("no suggestions left :(\n");
		return ;
	}
	wchar_t	*suggestions = (wchar_t *)calloc(count, sizeof(wchar_t));
	int	start = rand() % count;
	char	*str;

	suggestions[0] = kana;
	for (int i = 1; i < count; i++)
	{
		kana = random_kana();
		if (!strwchr(suggestions, kana, count))
			suggestions[i] = kana;
		else
			i--;
	}
	printf("its one of those: ");
	for (int i = 0; i < count; i++)
	{
		str = kana_to_romaji(suggestions[(i + start) % count]);
		printf(" %2s", str);
		free(str);
	}
	printf(" , %d suggestions left\n", wallet);
	free(suggestions);
}
		
static void	print_hint(wchar_t kana, int wallet)
{
	if (!(wallet >= 0))
	{
		printf("no hints left :(\n");
		return ;
	}
	char	*str = kana_to_romaji(kana);
	
	if (strlen(str) == 1)
		printf("its just one letter...,");
	else
		printf("the first letter is: %c ,", str[0]);
	printf(" %d hints left\n", wallet);
	free(str);
}

int	quizz(int question_count, int hint_wallet, int sugg_wallet)
{
	setlocale(LC_ALL, "");
	srand(time(NULL));
	char	*input = (char *)malloc(100 * sizeof(char));
	char	*str_kana_q;
	wchar_t	kana_q, kana_a;
	int	win = 0, fail = 0;

	//test_random(5000);
	printf("best if you resize your terminal to be 6 lines tall, zoom in\n");
	printf("enter \"!!\" for a suggestion (4 possibilities, 1 correct)\n");
	printf("enter \"??\" for a hint (first letter revealed)	\n");
	printf("going for %d questions, with %d hints and %d suggestions good luck\n\n", question_count, hint_wallet, sugg_wallet);
	while(win + fail < question_count)
	{
		kana_q = random_kana();
		str_kana_q = kana_to_romaji(kana_q);
		printf("do you know this letter??:)!!\n\n%lc\n\n>> ", kana_q);
		scanf("%s", input);
		if (strcmp(input, "!!") * strcmp(input, "??") == 0)
		{
			if (*input == '!')
				print_sugg(kana_q, 4, --sugg_wallet);
			else
				print_hint(kana_q, --hint_wallet);
			scanf("%s", input);
		}
		kana_a = romaji_to_kana(input);
		if (kana_a)
			printf("%lc\n", kana_a);
		else
			printf("thats not even a letter:(\n");
		if (kana_a == kana_q)
		{
			printf("yay:)                  ");
			win++;
		}
		else
		{
			str_kana_q = kana_to_romaji(kana_q);
			printf("wrong:( it was %3s,  %lc", str_kana_q, kana_q);
			free(str_kana_q);
			fail++;
		}
		printf("   %3d/%3d (%3d%%)\n", win, fail, (100 * win) / (win + fail));
	}
	free(input);
	char	*congrats = "<3!<3!<3 !!congrats!! <3!<3!<3\n";
	printf("%s", congrats);
	if (fail)
		printf("you got %d right out of %d\n", win, win + fail);
	else
		printf("big brain, all perfect\n");
	printf("%s", congrats);
	return (0);
}

int main(int argc, char **argv)
{
	int question_count, hint_wallet, sugg_wallet;

	if (argc >= 2)
		question_count = atoi(argv[1]);
	else
		question_count = 20;
	if (argc >= 3)
		hint_wallet = atoi(argv[2]);
	else
		hint_wallet = question_count / 5;
	if (argc >= 4)
		sugg_wallet = atoi(argv[3]);
	else
		sugg_wallet = question_count / 4;
	return (quizz(question_count, hint_wallet, sugg_wallet));
}