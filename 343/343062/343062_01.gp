\\ \r
\\ Nは使わない

default(realprecision, 150);

tan(Pi/24)
sqrt((2-sqrt(2+sqrt(3)))/(2+sqrt(2+sqrt(3))))
(sqrt(2)-1)*(sqrt(3)-sqrt(2))

\\ DATA用
x=tan(Pi/24);
for(n=0, 120, d=floor(x); if(n>0, print1(d, ", ")); x=(x-d)*10);