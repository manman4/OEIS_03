s = 0
(0..8).to_a.repeated_permutation(4){|a,b,c,d|
if a*a+2*b*b+3*c*c+4*d*d==64
  s += 1
  #p s
  puts "1*#{a}^2 + 2*#{b}^2 + 3*#{c}^2 + 4*#{d}^2 = 64." 
end
}
s = 0
(0..12).to_a.repeated_permutation(5){|a,b,c,d,e|
if a*a+2*b*b+3*c*c+4*d*d+5*e*e==125
  s += 1
  p s
  puts "1*#{a}^2 + 2*#{b}^2 + 3*#{c}^2 + 4*#{d}^2 + 5*#{e}^2 = 125." 
end
}
