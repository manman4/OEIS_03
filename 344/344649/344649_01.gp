\\ G.f. of column k: Sum_{i=1..2*k} (-1)^(i-1) * q^(i*(i-1)/2+k) / Product_{j=1..2*k-i} (1-q^j) for k > 0. 
T(n,k) = if(k==0, 0^n, my(q='q+O('q^(n+1))); polcoef(sum(i=1, 2*k, (-1)^(i-1) * q^(i*(i-1)/2+k) / prod(j=1, 2*k-i, (1-q^j))), n));

M=139;
cnt = 0;
for(n=0, M, for(k=0, n, write("b344649_1.txt", cnt, " ", T(n, k)); cnt++));