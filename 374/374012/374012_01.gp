a_vector(n, k=6) = my(v=vector(n), cnt=0, d=0, p=1, s=sum(j=1, sqrtnint(n, k), x^j^k)+x*O(x^n)); while(cnt<n, d++; p*=s; for(i=1, n, if(!v[i] && polcoef(p, i), v[i]=d; cnt++))); v;
a_vector(79)

a_vector(703)