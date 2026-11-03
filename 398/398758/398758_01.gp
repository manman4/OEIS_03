a061026(n) = my(m=1); while(eulerphi(m)%n, m++); m;
for(n=1, 100, print1(a061026(n), ", "));
isok(p) = isprime(p) && sumdiv(p-1, d, a061026(d)==p)==1;

for(n=1, 1321, if(isok(n),print1(n,", ")));

