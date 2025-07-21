M=49;

apr(n, p, r) = r*binomial(n*p+r, n)/(n*p+r);
a(n) = apr(n\7, 8, n%7+1);
for(n=0, M, print1(a(n),", "));