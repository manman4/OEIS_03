M=13;

a(n) = n! * polcoef(exp(-x) / (1 + lambertw(-n*x)), n);
for(n=0, M, print1(a(n),", "))
