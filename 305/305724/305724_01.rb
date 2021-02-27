def A305724(n)
  a, b, c, d, e, f, g, h, i, j, k = 1, 201, 7001, 104881, 927441, 5707449, 26986089, 104535009, 346615329, 1014889769, 2684641785
  ary = [1]
  while ary.size <= n
    a, b, c, d, e, f, g, h, i, j, k = b, c, d, e, f, g, h, i, j, k, 11*k - 55*j + 165*i - 330*h + 462*g - 462*f + 330*e - 165*d + 55*c - 11*b + a
    ary << a
  end
  ary
end
n = 10000
ary = A305724(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
