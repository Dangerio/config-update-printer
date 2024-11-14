# Start via `make test-debug` or `make test-release`
async def test_ping(service_client):
    response = await service_client.get('/ping')
    assert response.status_code == 200
