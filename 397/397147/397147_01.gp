a_vector(n) = my(v=vector(n)); v[1]=1; for(m=2, n, v[m]=sum(k=1, m-1, v[k]*v[m-k])+sum(r=1, m\3, (-1)^(r+1)*binomial(m-2*r, r)*v[m-2*r])); v;
a_vector(30)
