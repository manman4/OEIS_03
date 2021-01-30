(0..10).to_a.repeated_permutation(4){|a,b,c,d|
  puts "1* #{a}^2 + 2*#{b}^2 + 3*#{c}^2 + 4*#{d}^2 = 100" if a*a+2*b*b+3*c*c+4*d*d==100
}
