#!/usr/bin/env ruby
# frozen_string_literal: true

# For F(x) = x * g(x) with g(x) defined by g = 1 + x * g^m,
# compute the coefficients of F_k(x) / x in two ways:
# 1. direct truncated power series composition
# 2. the closed-form transition matrix
#
# The (n_index)-th value returned by closed_form_zero_to_n, with n_index = n + 1
# in 1-based language, is A_n:
#
#   A_n =
#     sum over 0 = x_0 <= x_1 <= ... <= x_{k-1} <= x_k = n of
#     product over j = 0, ..., k - 1 of
#       ((x_j + 1) / (m*x_{j+1} - (m - 1)*x_j + 1))
#       * binomial(m*x_{j+1} - (m - 1)*x_j + 1, x_{j+1} - x_j)

DEFAULT_N = 21
DEFAULT_K = 3
DEFAULT_M = 4

def binomial(n, r)
  return 0 if r.negative? || r > n

  r = [r, n - r].min
  value = 1
  1.upto(r) do |i|
    value = value * (n - r + i) / i
  end
  value
end

def zero_series(limit)
  Array.new(limit + 1, 0)
end

def series_mul(a, b, limit)
  result = zero_series(limit)

  0.upto(limit) do |i|
    next if a[i].zero?

    0.upto(limit - i) do |j|
      next if b[j].zero?

      result[i + j] += a[i] * b[j]
    end
  end

  result
end

def series_pow(series, exponent, limit)
  result = zero_series(limit)
  result[0] = 1
  base = series.dup
  power = exponent

  until power.zero?
    result = series_mul(result, base, limit) if power.odd?
    power >>= 1
    base = series_mul(base, base, limit) unless power.zero?
  end

  result
end

def series_compose(f_series, g_series, limit)
  result = zero_series(limit)
  g_power = zero_series(limit)
  g_power[0] = 1

  0.upto(limit) do |degree|
    coefficient = f_series[degree]
    unless coefficient.zero?
      0.upto(limit) do |j|
        next if g_power[j].zero?

        result[j] += coefficient * g_power[j]
      end
    end

    g_power = series_mul(g_power, g_series, limit) if degree < limit
  end

  result
end

def build_g_series(n, m)
  limit = n + 1
  g_series = zero_series(limit)
  g_series[0] = 1

  1.upto(n + 1) do
    g_power = series_pow(g_series, m, limit - 1)
    next_g = zero_series(limit)
    next_g[0] = 1

    0.upto(limit - 1) do |degree|
      next_g[degree + 1] = g_power[degree]
    end

    g_series = next_g
  end

  g_series
end

def f_k_div_x_direct(n, k, m)
  limit = n + 1
  return [1] + Array.new(n, 0) if k.zero?

  g_series = build_g_series(n, m)
  f_series = zero_series(limit)

  0.upto(n) do |degree|
    f_series[degree + 1] = g_series[degree]
  end

  f_iterated = f_series.dup
  2.upto(k) do
    f_iterated = series_compose(f_series, f_iterated, limit)
  end

  f_iterated[1..]
end

def transition_entry(row, col, m)
  return Rational(0, 1) if col < row

  numerator = m * col - (m - 1) * row + 1
  Rational(row + 1, numerator) * binomial(numerator, col - row)
end

def closed_form_zero_to_n(n, k, m)
  return [] if n.negative?

  size = n + 1
  transition = Array.new(size) do |row|
    Array.new(size) do |col|
      transition_entry(row, col, m)
    end
  end

  row_vector = Array.new(size, Rational(0, 1))
  row_vector[0] = Rational(1, 1)

  k.times do
    next_row = Array.new(size, Rational(0, 1))

    0.upto(size - 1) do |col|
      sum = Rational(0, 1)
      0.upto(col) do |mid|
        next if row_vector[mid].zero?

        sum += row_vector[mid] * transition[mid][col]
      end
      next_row[col] = sum
    end

    row_vector = next_row
  end

  row_vector.map do |value|
    raise "non-integer value detected: #{value}" unless value.denominator == 1

    value.numerator
  end
end

def validate_parameter_ranges(max_n, max_k, max_m)
  1.upto(max_m) do |m|
    0.upto(max_k) do |k|
      0.upto(max_n) do |n|
        direct_values = f_k_div_x_direct(n, k, m)
        closed_values = closed_form_zero_to_n(n, k, m)

        next if direct_values == closed_values

        warn "Mismatch detected:"
        warn "  n = #{n}, k = #{k}, m = #{m}"
        warn "  direct = #{direct_values.inspect}"
        warn "  closed = #{closed_values.inspect}"
        raise "validation failed"
      end
    end
  end
end

puts f_k_div_x_direct(DEFAULT_N, DEFAULT_K, DEFAULT_M).inspect
puts closed_form_zero_to_n(DEFAULT_N, DEFAULT_K, DEFAULT_M).inspect
validate_parameter_ranges(DEFAULT_N, DEFAULT_K, DEFAULT_M)