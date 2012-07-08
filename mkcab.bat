:: IMPORTANT - Must copy ActiveX OCX file to same directory as INF and CAB files
move .\release\fileactivex.ocx .
cabarc -s 6144 N fileactivex.cab fileactivex.ocx  fileactivex.inf
