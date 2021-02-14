def A(n)
  a, b, c, d, e = 36, 256, 2916, 38416, 527076
  cnt = 1
  ary = [36]
  while cnt < n
    a, b, c, d, e = b, c, d, e, 19*e - 76*d + 76*c - 19*b + a
    ary << a
    cnt += 1
  end
  ary
end

def B(n)
  a, b, c, d = 36, 256, 2916, 38416
  cnt = 1
  ary = [36]
  while cnt < n
    a, b, c, d = b, c, d, 18*d - 58*c + 18*b - a + 144
    ary << a
    cnt += 1
  end
  ary
end


n = 20
p a = A(n)
p a == B(n)


