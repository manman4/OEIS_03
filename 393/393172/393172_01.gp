\\ a(1) = 1; a(n) = Sum_{k=1..n-1} a(k)*a(n-k) + Sum_{r=1..floor(n/2)} (-1)^(r+1) * binomial(n-r,r) * a(n-r). 
a_vector(n) = my(v=vector(n)); v[1]=1; for(m=2, n, v[m]=sum(i=1, m-1, v[i]*v[m-i])+sum(r=1, m\2,(-1)^(r+1)*binomial(m-r, r)*v[m-r])); v;
a_vector(24)
