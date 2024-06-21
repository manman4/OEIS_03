default(parisize, 12000000000);

b(k, n) = my(q='q+O('q^(n+1))); polcoef((eta(q^2)^5/(eta(q)^2*eta(q^4)^2))^k/(1-q), n);
a(n) = b(3, 10^n);

for(n=0, 6, print1(a(n), ", "))

