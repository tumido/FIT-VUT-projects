#!/bin/bash
# =====================================================================
#          Verze:  1.0
#      Vytvoreno:  25.3.2014 12:41:20
#          Autor:  Tomas Coufal
#          Login:  xcoufa09
#        Projekt:  IOS - Projekt c.1
# =====================================================================
#

#
# ======================================================================
#            Script pro parsovani vystupu programu objdump
# ======================================================================
graph=false
plt=false
mode=0

#   Funkce pro vypis grafu
# ----------------------------------------------------------------------
# - printPlt() zajisti vypis funkci ze sdilenych knihoven
# - printGraph() vypise graf standardu CG, pokud je zapnuty prepinac
printPlt() { if [ $1 == false ]; then grep -v '@plt'; else { while read input; do echo $input; done } fi }
printGraph() { if [ $1 == true ]; then { echo "digraph CG {"; while read input; do echo $input | awk '{sub("@plt", "_PLT"); print $0";"}'; done; echo "}"; }; else { while read input; do echo $input; done } fi }

#   Errorove hlaseni
# ----------------------------------------------------------------------
printErr() { echo $1 >&2 ; echo "Usage: depcg.sh [-g] [-p] [-r FUNCTION_ID|-d FUNCTION_ID] FILE" >&2 ; exit 1; }

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
# - na zaklade prepinace modu vypise vysledek patricne magie v awk
# - vystup je triden abecedne
case $mode in
	0) objdump -d -j .text $1 | awk  '/.*>:$/,/^$/{if ($0 ~ />:/){sub(".*<","");sub(">:", "");a=$0}else{if ($0 ~ /.*callq.*<.*/ && $0 !~ /.*\+.*/){sub(".*<","");sub(">.*",""); print a " -> " $0}}}'  | printPlt $plt | sort -u | printGraph $graph ;;
	1) objdump -d -j .text $1 | awk -v fu="$data" '/.*'$data'>:/,/^$/{if ($0 ~ /callq/){sub(".*<",""); sub(">",""); print fu " -> "$0}}' | printPlt $plt | sort -u | printGraph $graph ;;
	2) objdump -d -j .text $1 | tac | awk -v fu="$data" '/callq.*'$data'>$/,/:$/{if ($0 ~ /.*:$/){sub(".*<","");sub(">:","");print $0" -> " fu}};' | printPlt $plt | sort -u | printGraph $graph ;;
	*) printErr "Fatal error, WTF!";;
esac
