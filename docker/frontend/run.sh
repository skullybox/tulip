echo "Running frontend image and mapping local port 4000 -> container:443"
docker run -d -p 4000:443 frontend
