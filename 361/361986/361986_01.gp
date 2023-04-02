a(n) = {
    v=[1, 3];
    if(n<3, 
        v[n],
        n^2 * sumdiv(n, d, if(d<n, (-1)^(n/d) * a(d) / d^2))
    )
};
for(n=1, 50, print1(a(n),", "))
