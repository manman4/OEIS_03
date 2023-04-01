a(n) = {
    v=[1, -5];
    if(n<3, 
        v[n],
        -n^2 * sumdiv(n, d, if(d<n, a(d) / d^2))
    )
}

for(n=1, 30, print1(a(n),", ")) 
