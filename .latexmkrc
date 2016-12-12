#!/usr/bin/env perl

$latex            = 'platex %O -halt-on-error -interaction=nonstopmode -file-line-error -shell-escape -synctex=1 %S';
$pdflatex         = 'pdflatex %O -shell-escape -synctex=1 %S';
$bibtex           = 'bibtex %O %B';
$dvipdf           = 'dvipdfmx %O -o %D %S';
$makeindex        = 'mendex %O -o %D %S';
$pdf_mode         = 3; # generates pdf via dvipdfmx

# Prevent latexmk from removing PDF after typeset.
# This enables Skim to chase the update in PDF automatically.
$pvc_view_file_via_temporary = 0;
