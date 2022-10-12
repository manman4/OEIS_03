M=22;

a(n) = if(n==0, 0, round(n!*polcoef(sin(sqrt(n)*(exp(x+x*O(x^n))-1))/sqrt(n), n)));
for(n=0, M, print1(a(n), ", "));
