\\ a(n) = (Re((1+2*i)^(2*n)) + 1)/2, where i = sqrt(-1).
a(n) = (real((1+2*I)^(2*n))+1)/2;
for(n=0, 24, print1(a(n),", "));
