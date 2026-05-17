\\ G.f. of column k: Sum_{i=1..2*k} (-1)^(i-1) * q^(i*(i-1)/2+k) / Product_{j=1..2*k-i} (1-q^j) for k > 0. 
\\ Sum_{n>=0} T(n,k) * q^n = (Product_{j>0} 1/(1-q^j)) * (Sum_{n>=0} A344649(n,k) * q^n).
T(n,k) = my(q='q+O('q^(n+1)), p=prod(k=1, n, 1/(1-q^k))); if(k==0, polcoef(p, n), polcoef(p * sum(i=1, 2*k, (-1)^(i-1) * q^(i*(i-1)/2+k) / prod(j=1, 2*k-i, (1-q^j))), n));
for(n=0, 20, for(k=0, n, print1(T(n,k),", ")));