# a(n) = 14*a(n-1) - 75*a(n-2) + 196*a(n-3) - 269*a(n-4) + 196*a(n-5) - 75*a(n-6) + 14*a(n-7) - a(n-8).
def a(n)
  ary = [0, 1, 6, 33, 174, 892, 4480, 22149]
  (8..n).each{|i|
    ary << 14*ary[-1] - 75*ary[-2] + 196*ary[-3] - 269*ary[-4] + 196*ary[-5] - 75*ary[-6] + 14*ary[-7] - ary[-8]
  }  
  ary
end

n = 500
ary = a(n)
(0..n).each{|i|
  print i
  print ' '
  puts ary[i]
}