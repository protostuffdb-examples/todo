// TODO copyright header

package todo.user;

import java.io.IOException;
import java.util.List;
import java.util.Properties;

import com.dyuproject.protostuff.Pipe;
import com.dyuproject.protostuff.RpcHeader;
import com.dyuproject.protostuff.RpcLogin;
import com.dyuproject.protostuff.RpcResponse;
import com.dyuproject.protostuff.RpcService;
import com.dyuproject.protostuff.RpcServiceProvider;
import com.dyuproject.protostuff.ds.ParamKey;
import com.dyuproject.protostuffdb.Datastore;
import com.dyuproject.protostuffdb.DatastoreManager;
import com.dyuproject.protostuffdb.WriteContext;

/**
 * User service provider.
 */
public class UserProvider extends RpcServiceProvider
{
    
    @Override
    public void fill(RpcService[] services, List<Integer> ids, 
            Datastore store, WriteContext context, Properties options, 
            DatastoreManager manager)
    {
        fill(services, ids, getClass());
        EntityRegistry.initSeq(store, context);
    }
    
    @Override
    public byte[] authenticate(RpcLogin login, Datastore store, WriteContext context)
    {
        return null;
    }

    public static final UserServices.ForUser FOR_USER = new UserServices.ForUser()
    {
        @Override
        public boolean generateInt(Datastore store, RpcResponse res,
                Pipe.Schema<EchoInt> resPipeSchema, RpcHeader header) throws IOException
        {
            res.output.writeInt32(EchoInt.FN_P, 123456789, false);
            return true;
        }
        
        @Override
        public String parseInt(EchoInt req, Datastore store, WriteContext context,
                RpcHeader header) throws IOException
        {
            return null;
        }
        
        @Override
        public boolean echoInt(EchoInt req, Datastore store, RpcResponse res,
                Pipe.Schema<EchoInt> resPipeSchema, RpcHeader header) throws IOException
        {
            EchoInt.getSchema().writeTo(res.output, req);
            return true;
        }
        
        @Override
        public boolean echoStr(EchoStr req, Datastore store, RpcResponse res,
                Pipe.Schema<EchoStr> resPipeSchema, RpcHeader header) throws IOException
        {
            EchoStr.getSchema().writeTo(res.output, req);
            return true;
        }
        
        @Override
        public boolean echoKey(ParamKey req, Datastore store, RpcResponse res,
                Pipe.Schema<ParamKey> resPipeSchema, RpcHeader header) throws IOException
        {
            ParamKey.getSchema().writeTo(res.output, req);
            return true;
        }
    };
}

