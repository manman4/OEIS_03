\\ G.f. of column k: Sum_{i=1..2*k} (-1)^(i-1) * q^(i*(i-1)/2+k) / Product_{j=1..2*k-i} (1-q^j) for k > 0. 
S(n,k) = if(k==0, 0^n, my(q='q+O('q^(n+1))); polcoef(sum(i=1, 2*k, (-1)^(i-1) * q^(i*(i-1)/2+k) / prod(j=1, 2*k-i, (1-q^j))), n));

M=13;
for(n=0, M, for(k=0, n, print1(S(n, k),", ")));
