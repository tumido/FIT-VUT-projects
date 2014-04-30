#!/bin/bash
# =====================================================================
#          Verze:  1.0
#      Vytvoreno:  26.3.2014 19:03:52
#          Autor:  Tomas Coufal
#          Login:  xcoufa09
#        Projekt:  IOS - Projekt c.1
# =====================================================================
#

#
# ======================================================================
#            Script pro parsovani vystupu programu nm
# ======================================================================

graph=false
plt=false
mode=0

#   Funkce pro vypis grafu
# ----------------------------------------------------------------------
# - graphSub() provadi substituci vyrazu dle specifikace
# - printGraph() vypise graf standargu GSYM pokud je zapnuty prepinac
#   grafu, nejdriv spojnice, pak definice bodu
graphSub() { while read input; do echo $input | awk '{node1=$1; node2=$3; gsub(/-[^>]/,"_");gsub(/\./,"D");gsub("+","P");gsub(/\(/,"[label=\"");gsub(/\)/,"\"];");gsub(/> .*\//, "> ");gsub(/^.*\//, ""); print "a "$0"\nb "$1" [label=\""node1"\"];\nb "$3" [label=\""node2"\"];"}' ; done | sort -u | awk '{sub($1" ", ""); print}'; }
printGraph() { if [ $1 == true ]; then { echo "digraph GSYM {"; graphSub ; echo "}"; }; else { while read input; do echo $input; done } fi }

#   Errorove hlaseni
# ----------------------------------------------------------------------
printErr() { echo $1 >&2 ; echo "Usage: depsym.sh [-g] [-r|-d OBJECT_ID] FILEs" >&2 ; exit 1; }

#   Spracovani argumentu
# ----------------------------------------------------------------------
while getopts :gpr:d: o
do
	case "$o" in
		g) graph=true;;
		p) plt=true;;
		r) mode=2
		   data=$OPTARG;;
		d) mode=1
		   data=$OPTARG;;
		:) printErr "Option -$OPTARG requires an argument.";;
		\?) printErr "Unknown option -$OPTARG.";;
	esac
done
((OPTIND--))
shift $OPTIND

#   Program
# ---------------------------------------------------------------------
# - vyrobi si pomocne promenne a pak na zaklade prepinace vypise
#   pozadovane kouzla
# - pomocne promenne ukladaji seznamy funkci a kde jsou definovane
# - script pak temito seznamy iteruje a hleda pozadovane zavislosti
defHere=$(for file in $*; do nm $file | awk '/.* T .*/ || /.* B .*/ || /.* C .*/ || /.* D .*/ || /.* G .*/{print $NF" '"$file"'"}'; done;)
undefHere=$(for file in $*; do nm $file | awk '/.* U .*/{print $NF" '"$file"'"}'; done;)

case $mode in
	0) for data in $*; do { for oneU in $(nm $data | awk '/.* U .*/{print $NF}'); do echo "$defHere" | grep '^'$oneU' ' | awk '{print "'$data' -> "$2" ("$1")"}'; done; } ; done | sort -u | printGraph $graph;;
	1) undef="$(nm $data | awk '/.* U .*/{print $NF}')"
		 for oneU in $undef; do echo "$defHere" | grep '^'$oneU' ' | awk '{print "'"$data"' -> "$2" ("$1")"}'; done | sort -u | printGraph $graph;;
	2) def="$(nm $data | awk '/.* T .*/ || /.* B .*/ || /.* C .*/ || /.* D .*/ || /.* G .*/{print $NF}')"
		 for oneU in $def; do echo "$undefHere" | grep '^'$oneU' ' | awk '{print $2" -> '"$data"' ("$1")"}'; done | sort -u | printGraph $graph;;
	*) printErr "Fatal error, WTF!";;
esac
