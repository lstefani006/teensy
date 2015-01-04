if &cp | set nocp | endif
let s:cpo_save=&cpo
set cpo&vim
map \rwp <Plug>RestoreWinPosn
map \swp <Plug>SaveWinPosn
vmap <silent> \tt :call AlignMaps#Vis("tt")
nmap \tt <Plug>AM_tt
vmap <silent> \tsq :call AlignMaps#Vis("tsq")
nmap \tsq <Plug>AM_tsq
vmap <silent> \tsp :call AlignMaps#Vis("tsp")
nmap \tsp <Plug>AM_tsp
vmap <silent> \tml :call AlignMaps#Vis("tml")
nmap \tml <Plug>AM_tml
vmap <silent> \tab :call AlignMaps#Vis("tab")
nmap \tab <Plug>AM_tab
vmap <silent> \m= :call AlignMaps#Vis("m=")
nmap \m= <Plug>AM_m=
vmap <silent> \tW@ :call AlignMaps#Vis("tW@")
nmap \tW@ <Plug>AM_tW@
vmap <silent> \t@ :call AlignMaps#Vis("t@")
nmap \t@ <Plug>AM_t@
vmap <silent> \t~ :call AlignMaps#Vis("t~")
nmap \t~ <Plug>AM_t~
vmap <silent> \t? :call AlignMaps#Vis("t?")
nmap \t? <Plug>AM_t?
vmap <silent> \w= :call AlignMaps#Vis("w=")
nmap \w= <Plug>AM_w=
vmap <silent> \ts= :call AlignMaps#Vis("ts=")
nmap \ts= <Plug>AM_ts=
vmap <silent> \ts< :call AlignMaps#Vis("ts<")
nmap \ts< <Plug>AM_ts<
vmap <silent> \ts; :call AlignMaps#Vis("ts;")
nmap \ts; <Plug>AM_ts;
vmap <silent> \ts: :call AlignMaps#Vis("ts:")
nmap \ts: <Plug>AM_ts:
vmap <silent> \ts, :call AlignMaps#Vis("ts,")
nmap \ts, <Plug>AM_ts,
vmap <silent> \t= :call AlignMaps#Vis("t=")
nmap \t= <Plug>AM_t=
vmap <silent> \t< :call AlignMaps#Vis("t<")
nmap \t< <Plug>AM_t<
vmap <silent> \t; :call AlignMaps#Vis("t;")
nmap \t; <Plug>AM_t;
vmap <silent> \t: :call AlignMaps#Vis("t:")
nmap \t: <Plug>AM_t:
vmap <silent> \t, :call AlignMaps#Vis("t,")
nmap \t, <Plug>AM_t,
vmap <silent> \t# :call AlignMaps#Vis("t#")
nmap \t# <Plug>AM_t#
vmap <silent> \t| :call AlignMaps#Vis("t|")
nmap \t| <Plug>AM_t|
vmap <silent> \T~ :call AlignMaps#Vis("T~")
nmap \T~ <Plug>AM_T~
vmap <silent> \Tsp :call AlignMaps#Vis("Tsp")
nmap \Tsp <Plug>AM_Tsp
vmap <silent> \Tab :call AlignMaps#Vis("Tab")
nmap \Tab <Plug>AM_Tab
vmap <silent> \TW@ :call AlignMaps#Vis("TW@")
nmap \TW@ <Plug>AM_TW@
vmap <silent> \T@ :call AlignMaps#Vis("T@")
nmap \T@ <Plug>AM_T@
vmap <silent> \T? :call AlignMaps#Vis("T?")
nmap \T? <Plug>AM_T?
vmap <silent> \T= :call AlignMaps#Vis("T=")
nmap \T= <Plug>AM_T=
vmap <silent> \T< :call AlignMaps#Vis("T<")
nmap \T< <Plug>AM_T<
vmap <silent> \T; :call AlignMaps#Vis("T;")
nmap \T; <Plug>AM_T;
vmap <silent> \T: :call AlignMaps#Vis("T:")
nmap \T: <Plug>AM_T:
vmap <silent> \Ts, :call AlignMaps#Vis("Ts,")
nmap \Ts, <Plug>AM_Ts,
vmap <silent> \T, :call AlignMaps#Vis("T,")
nmap \T, <Plug>AM_T,
vmap <silent> \T# :call AlignMaps#Vis("T#")
nmap \T# <Plug>AM_T#
vmap <silent> \T| :call AlignMaps#Vis("T|")
nmap \T| <Plug>AM_T|
map \Htd <Plug>AM_Htd
vmap <silent> \anum :call AlignMaps#Vis("anum")
nmap \anum <Plug>AM_anum
vmap <silent> \aenum :call AlignMaps#Vis("aenum")
nmap \aenum <Plug>AM_aenum
vmap <silent> \aunum :call AlignMaps#Vis("aunum")
nmap \aunum <Plug>AM_aunum
vmap <silent> \afnc :call AlignMaps#Vis("afnc")
nmap \afnc <Plug>AM_afnc
vmap <silent> \adef :call AlignMaps#Vis("adef")
nmap \adef <Plug>AM_adef
vmap <silent> \adec :call AlignMaps#Vis("adec")
nmap \adec <Plug>AM_adec
vmap <silent> \ascom :call AlignMaps#Vis("ascom")
nmap \ascom <Plug>AM_ascom
vmap <silent> \aocom :call AlignMaps#Vis("aocom")
nmap \aocom <Plug>AM_aocom
vmap <silent> \adcom :call AlignMaps#Vis("adcom")
nmap \adcom <Plug>AM_adcom
vmap <silent> \acom :call AlignMaps#Vis("acom")
nmap \acom <Plug>AM_acom
vmap <silent> \abox :call AlignMaps#Vis("abox")
nmap \abox <Plug>AM_abox
vmap <silent> \a( :call AlignMaps#Vis("a(")
nmap \a( <Plug>AM_a(
vmap <silent> \a= :call AlignMaps#Vis("a=")
nmap \a= <Plug>AM_a=
vmap <silent> \a< :call AlignMaps#Vis("a<")
nmap \a< <Plug>AM_a<
vmap <silent> \a, :call AlignMaps#Vis("a,")
nmap \a, <Plug>AM_a,
vmap <silent> \a? :call AlignMaps#Vis("a?")
nmap \a? <Plug>AM_a?
nmap gx <Plug>NetrwBrowseX
nnoremap <silent> <Plug>NetrwBrowseX :call netrw#NetrwBrowseX(expand("<cfile>"),0)
nmap <silent> <Plug>RestoreWinPosn :call RestoreWinPosn()
nmap <silent> <Plug>SaveWinPosn :call SaveWinPosn()
nmap <SNR>10_WE <Plug>AlignMapsWrapperEnd
map <SNR>10_WS <Plug>AlignMapsWrapperStart
let &cpo=s:cpo_save
unlet s:cpo_save
set autoindent
set backspace=indent,eol,start
set cinoptions=:0,g0
set fileencodings=ucs-bom,utf-8,default,latin1
set helplang=en
set hlsearch
set ignorecase
set ruler
set shiftwidth=4
set showmatch
set smartcase
set suffixes=.bak,~,.swp,.o,.info,.aux,.log,.dvi,.bbl,.blg,.brf,.cb,.ind,.idx,.ilg,.inx,.out,.toc,.png,.jpg
set tabstop=4
set window=36
" vim: set ft=vim :
