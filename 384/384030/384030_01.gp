c(n) = {
    my(sum = 0);
    for(i=0, n,
        for(j=0, n,
            for(k=0, n,
                for(l=0, n, 
                    if(i + j + k + l == 2*n,
                        sum += abs(stirling(n, i, 1) * stirling(n, j, 1) * stirling(n, k, 1) * stirling(n, l, 1));
                    )
                )
            )
        )
    );
    return(sum);
};

for(n=0, 20, print1(c(n),", "));
