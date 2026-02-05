\\ a(0) = 1, a(2*n) = a(n), a(2*n+1) = Sum_{i,j,k>=0 and i+j+k=n} a(i) * a(j) * a(k).
a_vector(n) = {
    my(v = vector(n + 1));
    v[1] = 1; 
    for(i = 1, n,
        if(i % 2 == 0,
            v[i+1] = v[i/2 + 1],
            my(k=i\2);
            my(s=0);
            for(p = 0, k,
                for(q = 0, k - p,
                    my(r = k - p - q);
                    s += v[p+1] * v[q+1] * v[r+1];
                );
            );
            v[i+1] = s;
        );
    );
    return(v);
}
print(a_vector(60));