# Number of 132-avoiding permutations of length 3*n composed only of 3-cycles.
#
# Generating function:
#   A(x) = 1 / sqrt(2*sqrt(1 - 4*x) - 1).
#
# Initial values:
#   a(0) = 1, a(1) = 2.
#
# Recurrence for n >= 2:
#   3*n*(n-1)*a(n)
#   - 2*(n-1)*(14*n-13)*a(n-1)
#   + 4*(4*n-5)*(4*n-7)*a(n-2) = 0.
#
# Equivalent closed form for n >= 1:
#   a(n) = (1/n) * sum_{k=1..n} k * C(2*k, k) * C(2*n-k-1, n-1).

module A132Avoiding3CyclesOnly
  module_function

  CACHE = [1, 2].freeze
  @cache = CACHE.dup

  def reset_cache!
    @cache = CACHE.dup
  end

  def a(n)
    raise ArgumentError, "n must be nonnegative" if n < 0

    ensure_terms(n)[n]
  end

  def sequence(limit)
    raise ArgumentError, "limit must be nonnegative" if limit < 0

    ensure_terms(limit)[0, limit + 1]
  end

  def a_direct(n)
    raise ArgumentError, "n must be nonnegative" if n < 0
    return 1 if n == 0
    return 2 if n == 1

    term = 2 * binomial(2 * n - 2, n - 1)
    sum = term

    1.upto(n - 1) do |k|
      num = term * 2 * (2 * k + 1) * (n - k)
      den = k * (2 * n - k - 1)
      term, rem = num.divmod(den)
      raise "nonintegral summand step at n=#{n}, k=#{k}" unless rem.zero?

      sum += term
    end

    value, rem = sum.divmod(n)
    raise "nonintegral final division at n=#{n}" unless rem.zero?

    value
  end

  def verify(limit)
    raise ArgumentError, "limit must be nonnegative" if limit < 0

    0.upto(limit) do |n|
      rec = a(n)
      dir = a_direct(n)
      raise "mismatch at n=#{n}: recurrence=#{rec}, direct=#{dir}" if rec != dir
    end
    true
  end

  def binomial(n, k)
    return 0 if k < 0 || k > n

    k = [k, n - k].min
    value = 1
    1.upto(k) do |i|
      value = value * (n - k + i) / i
    end
    value
  end

  def ensure_terms(limit)
    cache = @cache
    n = cache.length
    return cache if limit < n

    prev2 = cache[-2]
    prev1 = cache[-1]

    while n <= limit
      m = n
      num =
        2 * (m - 1) * (14 * m - 13) * prev1 -
        4 * (4 * m - 5) * (4 * m - 7) * prev2
      den = 3 * m * (m - 1)
      value, rem = num.divmod(den)
      raise "nonintegral recurrence step at n=#{m}" unless rem.zero?

      cache << value
      prev2 = prev1
      prev1 = value
      n += 1
    end

    cache
  end

  private_class_method :ensure_terms
end

def a(n)
  A132Avoiding3CyclesOnly.a(n)
end

def sequence(limit)
  A132Avoiding3CyclesOnly.sequence(limit)
end

def a_direct(n)
  A132Avoiding3CyclesOnly.a_direct(n)
end

if __FILE__ == $PROGRAM_NAME
  limit = ARGV[0] ? Integer(ARGV[0], 10) : 15
  ary = A132Avoiding3CyclesOnly.sequence(limit)
  puts ary.join(", ")
  ary.each_with_index do |v, n|
    puts "#{n} #{v}"
  end
end
