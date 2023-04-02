a(n) = {
    v=[1, 1];
    if(n<3, 
        v[n],
        n * sumdiv(n, d, if(d<n, (-1)^(n/d) * a(d) / d))
    )
};
for(n=1, 50, print1(a(n),", "))
