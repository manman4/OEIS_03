a(n) = {
    v=[1, 0];
    if(n<3, 
        v[n],
        sumdiv(n, d, if(d<n, (-1)^(n/d) * a(d)))
    )
};
for(n=1, 50, print1(a(n),", "))
