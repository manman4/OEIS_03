s = 0
(0..30).to_a.repeated_permutation(4){|a,b,c,d|
if a+4*b+9*c+16*d==30
  s += 1

  puts "1^2* #{a} + 2^2*#{b} + 3^2*#{c} + 4^2*#{d} = 30." 
end
}
s = 0
(0..36).to_a.repeated_permutation(3){|a,b,c,d|
if a+4*b+9*c==14
  s += 1
  p s
  puts "1^2* #{a} + 2^2*#{b} + 3^2*#{c} = 14" 
end
}
