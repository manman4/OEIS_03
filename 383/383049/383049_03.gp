\\ E.g.f. of column k: (1+x) * Sum_{j=0..k} Stirling2(k+1,j+1) * log(1+x)^j.
a(n, k) = my(x='x+O('x^(n+1))); n! * polcoef((1+x) * sum(j=0, k, stirling(k+1,j+1,2) * log(1+x)^j ), n);
for(n=0, M, for(k=0, n, print1(a(k, n-k), ", ")));
