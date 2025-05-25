pell(n) = ([2, 1; 1, 0]^n)[2, 1];
for(n=0, 20, print1(pell(n),", "))

a(n) = pell(n+1)*pell(n+2)-(-1)^n;
for(n=0, 20, print1(a(n),", "))  