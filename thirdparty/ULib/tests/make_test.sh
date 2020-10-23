#!/bin/sh

failed=0; all=0; xfail=0; xpass=0; skip=0;
list="$@"
if test -z "$list"; then
	list=`ls *.test`
fi
for tst in $list; do
	if	test -x "$tst"; then
		if ./$tst; then
			all=`expr $all + 1`
			echo "PASS: $tst"
		elif test $? -ne 77; then
			all=`expr $all + 1`
			failed=`expr $failed + 1`
			echo "FAIL: $tst"
		else
			skip=`expr $skip + 1`
			echo "SKIP: $tst"
		fi
	fi
done

if test "$failed" -eq 0; then
	if test "$xfail" -eq 0; then
		banner="All $all tests passed"
	else
		banner="All $all tests behaved as expected ($xfail expected failures)"
	fi
else 
	if test "$xpass" -eq 0; then 
		banner="$failed of $all tests failed"
	else 
		banner="$failed of $all tests did not behave as expected ($xpass unexpected passes)"
	fi
fi

dashes="$banner";
skipped="" 

if test "$skip" -ne 0; then 
	skipped="($skip tests were not run)"
	test `echo "$skipped" | wc -c` -le `echo "$banner" | wc -c` || dashes="$skipped";
fi

dashes=`echo "$dashes" | sed s/./=/g`
echo "$dashes"
echo "$banner" 
test -z "$skipped" || echo "$skipped"
test -z "$report" || echo "$report"
echo "$dashes"
if test -x "../.color"; then
	. ../.colors
	$NORMAL
fi
