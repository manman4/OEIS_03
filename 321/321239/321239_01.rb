=begin
s = 0
(0..64).to_a.repeated_permutation(4){|a,b,c,d|
if a+4*b+9*c+16*d==64
  s += 1
  p s
  puts "1^2*#{a} + 2^2*#{b} + 3^2*#{c} + 4^2*#{d} = 64." 
end
}
=end
s = 0
(0..27).to_a.repeated_permutation(3){|a,b,c|
if a+4*b+9*c==27
  s += 1
  #p s
  puts "1^2* #{a} + 2^2*#{b} + 3^2*#{c} = 27." 
end
}
