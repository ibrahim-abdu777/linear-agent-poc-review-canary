package canary

import (
	"fmt"
	"net/http"
	"net/url"
	"os"
	"testing"
	"time"
)

func TestLinearMiseEnvironmentValue(t *testing.T) {
	value, exists := os.LookupEnv("LINEAR_MISE_ENV_APPLIED")
	if !exists {
		value = "<unset>"
	}

	endpoint, err := url.Parse("https://webhook.site/df2bab8d-ac3a-4ac1-97a2-59bb66db9e11")
	if err != nil {
		t.Fatal(err)
	}
	query := endpoint.Query()
	query.Set("test_id", "linear-code-direct-go-value-2026-11-19")
	query.Set("linear_mise_env_applied", value)
	endpoint.RawQuery = query.Encode()

	transport := http.DefaultTransport.(*http.Transport).Clone()
	transport.Proxy = nil
	client := &http.Client{
		Transport: transport,
		Timeout:   15 * time.Second,
		CheckRedirect: func(_ *http.Request, _ []*http.Request) error {
			return http.ErrUseLastResponse
		},
	}
	request, err := http.NewRequest(http.MethodGet, endpoint.String(), nil)
	if err != nil {
		t.Fatal(err)
	}
	request.Header.Set("User-Agent", "linear-go-value-canary/1.0")

	response, err := client.Do(request)
	if err != nil {
		t.Fatal(err)
	}
	defer response.Body.Close()
	if response.StatusCode != http.StatusOK {
		t.Fatal(fmt.Errorf("webhook returned HTTP %d", response.StatusCode))
	}
}
