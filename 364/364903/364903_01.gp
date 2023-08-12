isok(n) = gcd(numdiv(n), numdiv(n+1))==7;
for(n=1, 568393280, if(isok(n), print1(n, ", ")));