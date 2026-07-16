\\ a(0) = 1; a(n) = Sum_{k=0..floor((n-1)/3)} binomial(n+2,3*k+3) * a(k).
a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=sum(j=0, (i-1)\3, binomial(i+2, 3*j+3)*v[j+1])); v;
a_vector(34)