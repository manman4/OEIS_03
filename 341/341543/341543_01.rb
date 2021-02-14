def A(n)
  a, b, c = 8, 36, 200
  cnt = 1
  ary = [8]
  while cnt < n
    a, b, c = b, c, 7 * c - 7 * b + a
    ary << a
    cnt += 1
  end
  ary
end

def B(n)
  a, b = 8, 36
  cnt = 1
  ary = [8]
  while cnt < n
    a, b = b, 6 * b - a - 8
    ary << a
    cnt += 1
  end
  ary
end


n = 20
p a = A(n)
p a == B(n)


