\\ a(n) = (1/(n+1)) * Sum_{k=0..n} binomial(n+1,k) * binomial(4*n+k+2,n-k).
a(n) = (1/(n+1)) * sum(k=0, n, binomial(n+1,k) * binomial(4*n+k+2,n-k));
for(n=0, 20, print1(a(n),", "));

\\ a(n) = (1/(n+1)) * [x^n] ((1-x+x^2) / (1-x)^5)^(n+1).
b(n) = my(x='x+O('x^(n+5))); polcoef( ((1-x+x^2)/(1-x)^5)^(n+1), n)/(n+1);
for(n=0, 100, print1(a(n)-b(n),", "));