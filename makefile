runner: runner.c fileops.c scanner.c parser.c release.c interpret.c namespace.c global.c
	gcc -g runner.c fileops.c scanner.c parser.c release.c interpret.c namespace.c global.c -o runner
random: random.c
	gcc random.c -o random