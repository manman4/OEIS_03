\\ \r
\\ Nは使わない

default(realprecision, 150);

imag(I^(1/16))
sin(Pi/32)
sqrt(2-sqrt(2+sqrt(2+sqrt(2))))/2

\\ DATA用
x=sin(Pi/32);
for(n=0, 120, d=floor(x); if(n>0, print1(d, ", ")); x=(x-d)*10);
