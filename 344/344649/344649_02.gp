\\ G.f. of column k: Sum_{i=1..2*k} (-1)^(i-1) * q^(i*(i-1)/2+k) / Product_{j=1..2*k-i} (1-q^j) for k > 0. 
T(n,k) = if(k==0, 0^n, my(q='q+O('q^(n+1))); polcoef(sum(i=1, 2*k, (-1)^(i-1) * q^(i*(i-1)/2+k) / prod(j=1, 2*k-i, (1-q^j))), n));

\\ G.f. of column k: Sum_{i=1..2*k} q^(i*(i-1)+k) * q_binomial(2*k-1,i-1) / Product_{j=1..i-1} (1-q^j) for k > 0.
q_binomial(n,r) = if(r < 0 || r > n, 0, prod(i=1, r, 1 - q^(n-i+1)) / prod(i=1, r, 1 - q^i));
S(n,k) = if(k==0, 0^n, my(q='q+O('q^(n+1))); polcoef(sum(i=1, 2*k, q^(i*(i-1)+k) * q_binomial(2*k-1,i-1) / prod(j=1, i-1, (1-q^j))), n));
for(n=0, 20, for(k=0, n, print1(S(n,k),", ")));

for(n=0, 30, for(k=0, n, print1(T(n,k)-S(n,k),", ")));