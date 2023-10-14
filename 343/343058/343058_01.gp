\\ \r
\\ Nは使わない

default(realprecision, 150);

tan(Pi/7)

\\ DATA用
x=tan(Pi/7);
for(n=0, 120, d=floor(x); if(n>0, print1(d, ", ")); x=(x-d)*10);