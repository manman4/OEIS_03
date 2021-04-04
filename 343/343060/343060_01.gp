\\ \r
\\ Nは使わない

default(realprecision, 150);

tan(Pi/16)
sqrt((2-sqrt(2+sqrt(2)))/(2+sqrt(2+sqrt(2))))
sqrt(4+2*sqrt(2))-sqrt(2)-1

\\ DATA用
x=tan(Pi/16);
for(n=0, 120, d=floor(x); if(n>0, print1(d, ", ")); x=(x-d)*10);