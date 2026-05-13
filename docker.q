pkg:([pcre2:([arch:"";            ubuntu:"";           rocky:"";           centos:"pcre2";      conda:""]);
      expat:([arch:"expat";       ubuntu:"libexpat1";  rocky:"expat";      centos:"";           conda:""]);
      blas: ([arch:"blas";        ubuntu:"libblas3";   rocky:"blas";       centos:"blas";       conda:"libblas"])])

/ https://stackoverflow.com/questions/78692851/could-not-retrieve-mirrorlist-http-mirrorlist-centos-org-release-7arch-x86-6
centos7patch:"sed -i 's/mirrorlist/#mirrorlist/g' /etc/yum.repos.d/CentOS-* && sed -i 's|#baseurl=http://mirror.centos.org|baseurl=http://vault.centos.org|g' /etc/yum.repos.d/CentOS-* && "

img:([arch:"archlinux:latest"; ubuntu:"ubuntu:focal"; rocky:"rockylinux:9.3"; centos:"centos:centos7"; conda:"continuumio/miniconda3"])
cmd:([arch:"pacman -Sy --noconfirm"; ubuntu:"apt-get update && apt-get install -y"; rocky:"yum install -y"; centos:centos7patch,"yum install -y"; conda:"conda install -y -c conda-forge"])
env:([arch:"";ubuntu:"";rocky:"";centos:"";conda:"CONDA_PREFIX=/opt/conda"])

S:first system@;U:S"id -u";u:S"id -un"

i:{s:` sv("FROM ",x;"RUN ",y;"ENV QHOME=/q QLIC=/l"," ",z;"RUN useradd -oNM -u ",U," ",u);S"printf \"",s,"\" | docker build -q -"}'[img;cmd{$[count p:trim" "sv value y;x," ",p;""]}'flip pkg;env]

QHOME:$[count s:getenv`QHOME;s;(getenv`HOME),"/q"]
QLIC:$[count s:getenv`QLIC;s;QHOME]
DIR:(last where"/"=s)#s:first -3#value{}

c:();r:();
{c,:enlist"docker run --rm -v '",QHOME,"':/q -v '",QLIC,"':/l -v '",DIR,"':/f -w /f -h `hostname` -u `id -un` -e QPATH=/f/out ",x," /q/l64/q test.q -test ",(" "sv(string key pkg),\:"/t.q")}each i;
/`:docker.txt 0: c;
{r,:system x}each c;
r:r[where not any like[r;] each ("Successfully loaded BLAS library*";"*all tests finished*")]
show r;
exit[0<count r]
